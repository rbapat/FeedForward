from newspaper import Article 
from textblob import TextBlob
import threading
import socket

def main():
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	server_address = ('localhost', 5555)

	while True:
		try:
			sock.connect(server_address)
			break
		except Exception:
			continue

	try:
		while True:
			data = sock.recv(256)
			article = Article(data.decode(), language = 'en')
			article.download()
			article.parse()
			article.nlp()

			summary = article.summary.replace('\n', '|').encode()

			iLen = str(len(summary)).encode()
			sock.sendall(iLen)
			sock.sendall(summary)

	finally:
		sock.close()

if __name__ == '__main__':
	main()