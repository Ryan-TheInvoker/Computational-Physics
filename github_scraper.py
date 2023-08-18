import requests
from bs4 import BeautifulSoup
import time

BASE_URL = "https://github.com/"


def get_user_info(username):
    url = f"https://github.com/{username}"
    response = requests.get(url)

    if response.status_code == 200: #what?
        
        soup = BeautifulSoup(response.content, 'html.parser')
        user_data = {}

        # Extract user data
        user_data['login'] = username
        user_data['id'] = soup.find('span', {'class': 'user-profile-bio'}).get('data-hovercard-url').split('/')[-1] if soup.find('span', {'class': 'user-profile-bio'}) else None
        user_data['avatar_url'] = soup.find('img', {'alt': f"@{username}"}).get('src')
        user_data['url'] = url
        user_data['html_url'] = url
        user_data['type'] = "User" 
        user_data['name'] = soup.find('span', {'class': 'p-name'}).text.strip() if soup.find('span', {'class': 'p-name'}) else None
        user_data['company'] = soup.find('span', {'itemprop': 'worksFor'}).text.strip() if soup.find('span', {'itemprop': 'worksFor'}) else None
        user_data['blog'] = soup.find('a', {'rel': 'me'}).get('href') if soup.find('a', {'rel': 'me'}) else None
        user_data['location'] = soup.find('span', {'itemprop': 'homeLocation'}).text.strip() if soup.find('span', {'itemprop': 'homeLocation'}) else None
        user_data['bio'] = soup.find('div', {'class': 'p-note'}).text.strip() if soup.find('div', {'class': 'p-note'}) else None
        user_data['public_repos'] = int(soup.find('span', {'class': 'Counter'}).text) if soup.find('span', {'class': 'Counter'}) else None
        user_data['followers'] = int(soup.find('span', {'href': f'/{username}?tab=followers'}).find('span').text)
        user_data['following'] = int(soup.find('span', {'href': f'/{username}?tab=following'}).find('span').text)

        return user_data
    else:
        return None

def get_repository_info(username):
    url = f"https://github.com/{username}?tab=repositories"
    response = requests.get(url)

    if response.status_code == 200:
        soup = BeautifulSoup(response.content, 'html.parser')
        repos = []

        for repo in soup.find_all('li', {'itemprop': 'owns'}):
            repo_data = {}
            repo_data['name'] = repo.find('a', {'itemprop': 'name codeRepository'}).text.strip()
            repo_data['full_name'] = f"{username}/{repo_data['name']}"
            repo_data['html_url'] = f"https://github.com/{username}/{repo_data['name']}"
            # Similarly, other details can be added by scraping the necessary elements
            repos.append(repo_data)

        return repos
    else:
        return None

# To handle rate limits, introduce a delay
def safe_request(user_or_repo_name, is_repo=False):
    time.sleep(1)  # delay for a second
    return get_repository_info(user_or_repo_name) if is_repo else get_user_info(user_or_repo_name)
