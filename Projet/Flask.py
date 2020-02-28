from flask import Flask
from flask import request
from flask import jsonify
import json

app = Flask(__name__)


@app.route("/", methods=['POST'])
def data():
	body = request.json
	if body == None:
		return "not a json"
	else:   
		with open('data.json', 'w') as outfile:
			json.dump(body, outfile)
			print(body)
	return jsonify(body) 

app.run(debug=True)