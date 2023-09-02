import requests
from bs4 import BeautifulSoup
import time


BASE_URL = "https://github.com/" 

###___Helper Methods___###





def is_org(soup):

    return soup.find('div', {'itemscope': '', 'itemtype': 'http://schema.org/Organization'})




#assists in getting the necessary information for a repo
def get_detailed_repo_info(username, repo_name):

    url = f"https://github.com/{username}/{repo_name}"
    response = requests.get(url) #headers?

    ###PAYLOAD for repo###
    url_search = f"https://github.com/search?q=repo:{username}/{repo_name}&type=repositories"
    response_search = requests.get(url_search)
    data = {}
    if response_search.status_code == 200:

        search_soup = BeautifulSoup(response_search.content, 'html.parser')
        
        if '"language":' in search_soup.text:
            
            language_element = search_soup.text.split('"language":')[1].split(',')[0].strip('"')
            if language_element != "null":
                data['language'] = language_element
            else: data['language'] = None
        else: data['language'] = None



    if response.status_code == 200:
        soup = BeautifulSoup(response.content, 'html.parser')
    

        #repo id not working?
        data['id'] = int(soup.find("meta", {'name':"octolytics-dimension-repository_id"}).get("content")) if soup.find("meta", {'name':"octolytics-dimension-repository_id"}) else None

        data['has_projects'] = bool(soup.find('a', {'id': 'projects-tab'}))
        
        data['has_discussions'] = bool(soup.find('a', {'id': 'discussions-tab'}))
        
        data['has_issues'] = bool(soup.find('a', {'id': 'issues-tab'}))

    

        

        #data['has_issues']  = bool(soup.find('a', href=lambda href: (href and "issues" in href)))


        #diving into issues page 
        url_issues = f"https://github.com/{username}/{repo_name}/issues"
        response1 = requests.get(url_issues)
        if response1.status_code == 200:
            soup_issues  = BeautifulSoup(response1.content, 'html.parser')
            
            issues_element = soup_issues.find('a', {'class': 'btn-link selected', 'data-ga-click': 'Issues, Table state, Open'})
            
            if issues_element:
                
                data['open_issues_count'] = int(''.join(filter(str.isdigit, issues_element.text.strip().replace(',', ''))))

            else: data['open_issues_count'] = 0

        #pulls
        pulls = soup.find('span',{'id':'pull-requests-repo-tab-count'}).text
        

        if 'k' in  pulls:
            url_pulls = f"https://github.com/{username}/{repo_name}/pulls"
            response_pulls = requests.get(url_pulls)
            
            if response_pulls.status_code == 200:
                
                soup_pulls  = BeautifulSoup(response_pulls.content, 'html.parser')
                #TODO pulls = 
        else: 
            data['open_issues_count'] = data['open_issues_count'] + int(pulls)

    # Scrape topics ; TODO: working for orgz but not users
        topics = []
        for topic in soup.find_all('a', {'class': 'topic-tag'}):
            topics.append(topic.text.strip())
            topics.sort()
        data['topics'] = topics

        

        #Default Branch #TODO: none or master?
        #default_branch_element_out = soup.find('div',{'class','position-realative'})
        default_branch_element = soup.find('span', {'class': 'css-truncate-target'})
        if default_branch_element:
            default_branch = default_branch_element.text.strip()
        else:
            default_branch = 'master'

        data['default_branch'] = default_branch


        element1 =soup.find('div',{'class','BorderGrid BorderGrid--spacious'})
        element2 = element1.find('a',{'class','text-bold'})
        if element2:
            data['homepage'] = element2.text.strip()
        else: data['homepage'] = None
        
        

     






        return data
    
    else: return []

###___Big Boys___###
def scrape_user(username,soup,url):
    user_data = {}
    user_data['login'] = username

    user_data['id'] = int(soup.find('meta', attrs={'name': 'octolytics-dimension-user_id'}).get('content'))

    if soup.find('img', {'alt': f"@{username}"}):
        av_url =soup.find('img', {'alt': f"@{username}"}).get('src') 
    
        user_data['avatar_url'] =  av_url.replace("?s=64&", "?")
    else:user_data['avatar_url'] = None


    user_data['url'] = f"https://api.github.com/users/{username}"
    user_data['html_url'] = url
    user_data['type'] = "User"

    user_data['name'] = soup.find('span', {'class': 'p-name'}).text.strip() if soup.find('span', {'class': 'p-name'}) else None

    
    if soup.find('a', class_='user-mention notranslate'):
        user_data['company'] = soup.find('a', class_='user-mention notranslate').text 

    elif soup.find('span',class_='p-org'):
        user_data['company'] = soup.find('span',class_='p-org').text
    else: None

    user_data['blog'] = soup.find('a', {'rel': 'me'}).get('href') if soup.find('a', {'rel': 'me'}) else None
    user_data['location'] = soup.find("span", class_="p-label").text if soup.find("span", class_="p-label") else None


    user_data['bio'] = soup.find('div', class_='user-profile-bio').text if soup.find('div', class_='user-profile-bio') else None #TODO:removed strip

    user_data['twitter_username'] = soup.find('a', {'data-hovercard-type': 'twitter'}).text.strip() if soup.find('a', {'data-hovercard-type': 'twitter'}) else None

    
    if soup.find('a', {'href': f'/{username}?tab=repositories'}):
        user_data['public_repos'] = int(soup.find('a', {'href': f'/{username}?tab=repositories'}).find('span').text.replace(',', '')) 
    else: 
        user_data['public_repos'] = None

    
   
    url_followers = f"https://github.com/search?q=user%3A{username}&type=users"
    
    response_followers = requests.get(url_followers)
    fsoup = BeautifulSoup(response_followers.content,'html.parser')

    if '"followers":' in fsoup.text:

        followers = fsoup.text.split(',')[6].split(':')[1]
    else: followers = 0

    if followers and followers != "null":
        user_data['followers'] = int(followers)
    else: user_data['followers'] = 0 

    # following
    #TODO: idk if it works
    if soup.find('a', href=lambda x: x and x.endswith('?tab=following')):
        user_data['following'] = int(soup.find('a', href=lambda x: x and x.endswith('?tab=following')).text.split('\n')[1].strip())
    else: 
        user_data['following'] = 0

    # twitter
    if soup.find('a', href=lambda href: (href and "twitter.com" in href)):
        user_data['twitter_username'] = soup.find('a', href=lambda href: (href and "twitter.com" in href)).text.replace('@','')
    else: 
        user_data['twitter_username'] = None

    return user_data


def scrape_organization(org_name,soup,url):
    org_data = {}

    
    org_data['name'] = soup.find('h1',{'class':'h2 lh-condensed'}).text.strip() if soup.find('h1',{'class':'h2 lh-condensed'}) else None#soup.find('span', {'class': 'org-name'}).text.strip() if soup.find('span', {'class': 'org-name'}) else None 
    org_data['login'] = org_name
    
    
    # Scrape Organization ID
    if soup.find('meta', {'name': 'hovercard-subject-tag'}):

        org_data['id']  = int(soup.find('meta', {'name': 'hovercard-subject-tag'}).get('content').split(":")[1].strip())

    else: org_data['id']  = None


    # Scrape Organization's avatar URL
    av_url = soup.find('img', {'alt': f"@{org_name}"}).get('src')
    
    if av_url:
        org_data['avatar_url'] = av_url.replace("?s=200&", "?")
    else: None

    # Scrape Organization's description
    main_container = soup.find('div', {'itemtype': 'http://schema.org/Organization'})
    
# Then, within that container, find the specific bio div
    if main_container:
        bio_div = main_container.find('div', {'class': 'color-fg-muted'})
        
        if bio_div:
            org_data['bio'] = bio_div.text.strip()

    else: org_data['bio'] = None
    #org_data['bio'] = soup.find('div',class_= 'color-fg-muted').text#soup.find('div', {'class': 'org-description'}).text.strip() if soup.find('div', {'class': 'org-description'}) else None

    # Scrape Organization's website
    org_data['twitter_username'] = soup.find('a', {'rel': 'nofollow me'}).text.strip('@') if soup.find('a', {'rel': 'me'}) else None

    # Scrape Location
    org_data['location'] = soup.find('span', {'itemprop': 'location'}).text.strip() if soup.find('span', {'itemprop': 'location'}) else None

    # Scrape Email
    org_data['email'] = soup.find('a', {'itemprop': 'email'}).text.strip() if soup.find('a', {'itemprop': 'email'}) else None

    org_data['type'] = 'Organization'
    org_data['following'] = 0
    org_data['blog'] = soup.find('a',{'rel': 'nofollow'}).text if soup.find('a',{'rel': 'nofollow'}) else None
    # Scrape Number of Repositories
    
    #print(soup.find('span', {'class': 'Counter'}))
    #org_data['public_repos'] = int(soup.find('span', {'class': 'Counter'}).text.strip().replace(',', '')) if soup.find('span', {'class': 'Counter'}) else None


    
    
    org_data['url'] = f"https://api.github.com/users/{org_name}"
    org_data['html_url'] = url

    url_followers = f"https://github.com/search?q=user%3A{org_name}&type=users"
    response_followers = requests.get(url_followers)
    fsoup = BeautifulSoup(response_followers.content,'html.parser')
    
    try:
        fsoup_list = fsoup.text.split(',')
        if len(fsoup_list) > 6:
            followers_part = fsoup_list[6]
            followers_list = followers_part.split(':')
            if len(followers_list) > 1:
                followers = followers_list[1]
            else:
                followers = 0  
        else:
            followers = 0  
    except IndexError:
        followers = 0      

    org_data['followers'] = int(followers)





    # followers = fsoup.text.split(',')[6].split(':')[1]
    # if followers and followers != "null":

    #     org_data['followers'] = int(fsoup.text.split(',')[6].split(':')[1])
    # else: org_data['followers'] = 0


    return org_data






def get_user_info(username):
    url = f"https://github.com/{username}"
    
    headers = {
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
    }
    response = requests.get(url, headers=headers) #what are the headers for?


    if response.status_code == 200:
        soup = BeautifulSoup(response.content, "html.parser")
        user_data = {}

        if is_org(soup):
            user_data = scrape_organization(username,soup,url) 
        else:
            user_data = scrape_user(username,soup,url)
        
           
      
            
        
        #if   #:

        return user_data
        
    else:
        return None







# def get_last_page_num(username_or_org):
#     url = f"https://github.com/{username_or_org}?tab=repositories"
#     response = requests.get(url)
#     soup = BeautifulSoup(response.content, 'html.parser')
    
#     #print(pagination_div)
#     if pagination_div:
#         page_links = pagination_div.find_all('a')
        
#         if len(page_links)>1:
#             last_page_number = int(page_links[-2].text) 
#         else:
#             last_page_number= 2
#     else:
#         last_page_number = 1
    
#     return last_page_number





#Returns the repository information
def get_repository_info(username):
    url = f"https://github.com/{username}?tab=repositories"
    
    headers = {
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
    }
    

    repos = []
    page = 0
    next_page = True
    
    while next_page:

        #print("iteration")

        page+=1
        url = f"https://github.com/{username}?tab=repositories&page={page}"
        response = requests.get(url, headers=headers)

        if response.status_code == 200:

            soup = BeautifulSoup(response.content, 'html.parser')
            


            #TODO: if soup is empty i.e. no repos --> end method
            
            if soup.find('li', {'itemprop': 'owns'}) or soup.find('li', {'class': 'Box-row'}):
                
                repo_list = []
                #repos = []
                user_data = get_user_info(username)
                user_id = user_data['id'] if user_data else None


                isOrg = is_org(soup)
                
                
                if isOrg:
                    repo_list = soup.find_all('li', {'class': 'Box-row'})
                else:
                    repo_list = soup.find_all('li', {'itemprop': 'owns'})
                


                for repo in repo_list:
                    
                    repo_data = {}

                    repo_data['name'] = repo.find('a', {'itemprop': 'name codeRepository'}).text.strip()
                    repo_data['full_name'] = f"{username}/{repo_data['name']}"
                    repo_data['html_url'] = f"https://github.com/{username}/{repo_data['name']}"
                    repo_data['description'] = repo.find('p', {'itemprop': 'description'}).text.strip() if repo.find('p', {'itemprop': 'description'}) else None
                    
                    repo_data['url'] = f"https://api.github.com/repos/{username}/{repo_data['name']}"
                    
                
                    
                    

                    repo_data['owner'] = {"login": username,"id": user_id } #finding the ID might be difficult

                    #stargazers
                    stargazers_element = repo.find('a', href=f'/{username}/{repo_data["name"]}/stargazers')
                    if stargazers_element:
                        repo_data['stargazers_count'] = int(stargazers_element.text.strip().replace(',', '')) 
                    else:
                        repo_data['stargazers_count'] = 0
                    
                    repo_data['watchers_count'] = repo_data['stargazers_count']
                    

                    

                    #private
                    privacy_element = repo.find('span', {'class': 'Label'})
                    if privacy_element:
                        privacy_status = privacy_element.text.strip()
                        if privacy_status == 'Private':
                            repo_data['private'] = True
                        else:
                            repo_data['private'] = False
                    else:
                        repo_data['private'] = None
                    
                    
                    repo_data['homepage'] = repo.find('a', {'class': 'repo-homepage-link'})
                    
                    archived_div = repo.find('div', {'class': 'repository-archived-banner'})
                    repo_data['archived'] = bool(archived_div)

                    #forks_count
                    name = repo_data['name']
                    span_element = repo.find('a', {'href': f'/{username}/{name}/forks'})
                    forks_count = span_element if span_element else None
                    #forks_count = soup.find('a',{'href':f'/{username}/{name}/forks'})
                    if forks_count:
                       
                            repo_data['forks_count'] = int(forks_count.text.strip().replace(",",""))
                    else: repo_data['forks_count'] = 0

                    #fork
                    fork_element = repo.find('span', {'class': 'f6 color-fg-muted mb-1'})
                    
                    #print(fork_element)
                    if fork_element:
                         #fork = fork_element.text
                        repo_data['fork'] = True
                    else: repo_data['fork'] = False

                    #pushed_at
                    #if pushed_at_found == False:
                    if repo.find('relative-time'):
                        pushed_element = repo.find('relative-time').get('datetime')
                        repo_data['pushed_at'] = pushed_element

                    else: repo_data['pushed_at'] = None

                    

                    detailed_info = get_detailed_repo_info(username, repo_data['name'])
                    repo_data.update(detailed_info)

                    repos.append(repo_data)

        
        if soup.find('span',{'class','next_page disabled'}): 
            next_page = False
            
        elif soup.find('a',{'class','next_page'}):
            next_page = True
            
        else: 
            
            next_page = False

#TODO: commenting out these lines might mess with empty repos
        #else:  #if no repos : then repos will be empty so just return 
            #return repos
    #sorted_repos = sorted(repos.items())
    sorted_repos = sorted(repos, key=lambda k: k['name'].lower())
    
    return sorted_repos #was None



# To handle rate limits, introduce a delay
def safe_request(user_or_repo_name, is_repo=False):
    time.sleep(1)  # delay for a second
    return get_repository_info(user_or_repo_name) if is_repo else get_user_info(user_or_repo_name)



#testing baby
#print(get_repository_info("shihamsam"))

