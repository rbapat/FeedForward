from newspaper import Article 
import threading
import socket



def fetch_data(link):
	article = Article(link, language = 'en')
	article.download()
	article.parse()
	article.nlp()

	summary = article.summary.replace('\n', '|').encode()
	return summary

def send_data(data, sock):
	iLen =	str(len(data)).encode()
	sock.sendall(iLen)
	sock.sendall(data)

def main():
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	server_address = ('localhost', 5555)

	while True:
		try:
			sock.connect(server_address)
			break
		except Exception:
			continue

	lastCmd = ''

	try:
		
		while True:
			data = sock.recv(256)

			send_data(fetch_data(data.decode()), sock)

	finally:
		sock.close()

if __name__ == '__main__':
	main()