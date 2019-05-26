import feedparser
import pandas as pd
# Imports the Google Cloud client library
from google.cloud import language
from google.cloud.language import enums
from google.cloud.language import types
import sys
import six
import os
from google.oauth2 import service_account


class BiasAnalyzer:
    def __init__(self):
        self.political_keyword_dict = {}
        self.keyword_list = []
        self.readKeywords("politcal_keywords.txt")
        self.createKeywordList()

    def readKeywords(self, keyword_file):
        my_file = open(keyword_file, 'r')
        political_keywords = my_file.readlines();
        temp = [];

        for line in political_keywords:
            temp.append(line.partition('\n')[0].split(','))
        political_keywords = temp
        self.political_keyword_dict = {'Conservative': [], 'Liberal': []}
        for keyword in political_keywords:
            if keyword[1] == 'Liberal':
              self.political_keyword_dict['Liberal'].append(keyword[0])
            elif keyword[1] == 'Conservative':
                self.political_keyword_dict['Conservative'].append(keyword[0])

    def createKeywordList(self):
        for vals in self.political_keyword_dict.values():
            for word in vals:
                self.keyword_list.append(word)

    # print(keyword_list)


    # copied this from Google Cloud Platform Entity Sentiment Analysis Tutorial and edited it to reflect political keywords and bias
    def entity_sentiment_text(self, text):
        """Detects entity sentiment in the provided text."""
        credentials = service_account.Credentials.from_service_account_file("key.json")
        client = language.LanguageServiceClient(credentials=credentials)

        if isinstance(text, six.binary_type):
            text = text.decode('utf-8')

        document = types.Document(
            content=text.encode('utf-8'),
            type=enums.Document.Type.PLAIN_TEXT)

        # Detect and send native Python encoding to receive correct word offsets.
        encoding = enums.EncodingType.UTF32
        if sys.maxunicode == 65535:
            encoding = enums.EncodingType.UTF16

        # MY STUFF
        #keywords =
        result = client.analyze_entity_sentiment(document, encoding)

        liberal_score = 0
        liberal_words = 0
        conservative_score = 0
        conservative_words = 0

        for entity in result.entities:
            for mention in entity.mentions:
                if entity.name in self.keyword_list:
                    print("    ", entity.name, "Magnitude:", mention.sentiment.magnitude, "Sentiment:", mention.sentiment.score)
                    if entity.name in list(self.political_keyword_dict.values())[1] and mention.sentiment.score > 0:
                        liberal_score += abs(mention.sentiment.score)
                        liberal_words += 1
                    elif entity.name in list(self.political_keyword_dict.values())[0] and mention.sentiment.score < 0:
                        liberal_score += mention.sentiment.score
                        liberal_words += 1
                    elif entity.name in list(self.political_keyword_dict.values())[0] and mention.sentiment.score > 0:
                        conservative_score += mention.sentiment.score
                        conservative_words += 1
                    elif entity.name in list(self.political_keyword_dict.values())[1] and mention.sentiment.score < 0:
                        conservative_score += abs(mention.sentiment.score)
                        conservative_words += 1

                    # print(u'  Begin Offset : {}'.format(mention.text.begin_offset))
                    # print(u'  Content : {}'.format(mention.text.content))
                    # print(u'  Magnitude : {}'.format(mention.sentiment.magnitude))
                    # print(u'  Sentiment : {}'.format(mention.sentiment.score))
                    # print(u'  Type : {}'.format(mention.type))

        if conservative_words > 0:
            conservative_score = conservative_score / conservative_words
        if liberal_words > 0:
            liberal_score = liberal_score / liberal_words

        if abs(conservative_score - liberal_score) < .2:
            return "Moderate or Not Enough Info"
        elif (conservative_score - liberal_score) > .2 and (conservative_score - liberal_score) < .7:
            return "Conservative"
        elif (liberal_score - conservative_score) > .2 and (liberal_score - conservative_score) < .7:
            return "Liberal"
        elif (conservative_score - liberal_score) > .7:
            return "Very Conservative"
        elif (liberal_score - conservative_score) > .7:
            return "Very Liberal"

        print(conservative_score)
        print(liberal_score)

        #     print('Mentions: ')
        #     print(u'Name: "{}"'.format(entity.name))

        #     print(u'Salience: {}'.format(entity.salience))
        #     print(u'Sentiment: {}\n'.format(entity.sentiment))

    # takes in name of .txt file with article, returns Very Liberal, Liberal, Moderate, Conservative, or Very Conservative
    # based on positive/negative sentiments toward certain keywords like social security, amnesty, free market, climate change, gun control, etc.
    def analyze_bias(self, filename):
        file = open(filename, 'r')
        print(self.entity_sentiment_text(''.join(file.readlines())))
        print()


test_analyzer = BiasAnalyzer()
print("Test Article")
test_analyzer.analyze_bias("test_article")
print("Left Article 1")
test_analyzer.analyze_bias("left1")
print("Left Article 2")
test_analyzer.analyze_bias("left2")
print("Right Article 1")
test_analyzer.analyze_bias("right1")
print("Right Article 2")
test_analyzer.analyze_bias("right2")
