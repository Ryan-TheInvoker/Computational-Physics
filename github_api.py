import os
from flask import Flask, jsonify, request
from github_scraper import get_user_info, get_repository_info

app = Flask(__name__)

# Define the base URL for the API
base_url = "/api/v1"

@app.route(base_url + "/users/<username>", methods=["GET"])
def get_user(username):
    try:
        user_data = get_user_info(username)
        if user_data:
            return jsonify(user_data), 200
        else:
            return jsonify({"message": f"No data found for username: {username}"}), 404
    except Exception as e:
        return jsonify({"message": f"Error fetching user data: {str(e)}"}), 500

@app.route(base_url + "/users/<username>/repos", methods=["GET"])
def get_repository(username):
    try:
        repo_data = get_repository_info(username)
        if repo_data:
            return jsonify(repo_data), 200
        else:
            return jsonify({"message": f"No repositories found for username: {username}"}), 404
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
    port = int(os.environ.get("GITHUB_API_PORT", 5000))
    app.run(host="0.0.0.0", port=port)
