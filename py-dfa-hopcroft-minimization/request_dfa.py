import requests

url = 'http://lfa.ddns.net/rdfa'

states = int(raw_input("States: "))
letters = int(raw_input("Letters: "))
finals = int(raw_input("Finals: "))
transitions = int(raw_input("Transitions: "))
difficulty = raw_input("Difficulty: ")

response = requests.post(url, data={'states':states, 'letters':letters, \
					'finals':finals, 'transitions':transitions, \
					'test_file':difficulty })

if response.status_code != 200:
	print("Request failed, status code: " + str(response.status_code))
	print(response.text)

with open('input.txt', 'w') as input_file:
	input_file.write(response.text)
	input_file.close()
