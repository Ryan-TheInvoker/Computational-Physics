import os
from flask import Flask, jsonify
from github_scraper import get_user_info, get_repository_info

app = Flask(__name__)

# Define the base URL for the API
#base_url = "/api/v1"

#decorator tells the Flask to execute the get_user function when some accesses the url /api/v1/users/{username}




@app.route(  "/users/<username>", methods=["GET"])  # <username> is a dynamic segment which captures whatever is input there and passes it to the function
def get_user(username):  # Added missing username parameter
    try:
        user_data = get_user_info(username)  # Assuming get_user_info is a function you've defined elsewhere to fetch the user data
        if user_data:
            return jsonify(user_data), 200  # if not empty, it sends the data as a JSON response with the status code '200'
        else:
            return jsonify({"message": f"Not Found"}), 404  # 404similar except if user data is empty

    except Exception as e:  # Added missing except block
        return jsonify({"message": f"Error fetching user data: {str(e)}"}), 500  # this is the error caught in the try block








@app.route( "/users/<username>/repos", methods=["GET"])
def get_repository(username):
    try:
        repo_data = get_repository_info(username)
        if repo_data:
            return jsonify(repo_data), 200
        else:
            return jsonify(repo_data), 200  #was 404 & an error message
    except Exception as e:
        return jsonify({"message": f"Error fetching repository data: {str(e)}"}), 500



@app.errorhandler(404)
def not_found(e):
    return jsonify({"message": "Endpoint not found"}), 404


@app.errorhandler(405)
def method_not_allowed(e):
    return jsonify({"message": "Method not allowed"}), 405



@app.errorhandler(500)
def internal_server_error(e):
    return jsonify({"message": "Internal server error"}), 500

if __name__ == "__main__":
    #port = int(os.environ.get("GITHUB_API_PORT", 5000))
    port = int(os.environ.get('GITHUB_API_PORT',5000))
    app.run(host="0.0.0.0",port=port) #, port=port
