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


# class News:
#     def __init__(self, name):
#         self.name = name
#         self.articles = {}
#
#     # parameters are all strings
#     def addArticle(self, title, summary, url):
#         self.articles[title] = [summary, url]
#
#
# # news outlets
# cnn = News("CNN")
#
# usa_today = News("USA Today")
#
#
# # cnn articles
# cnn_rss = feedparser.parse("http://rss.cnn.com/rss/cnn_topstories.rss")
#
# for i in range(0, 10):
#     the_summary = cnn_rss['entries'][i]['summary'].partition('<')[0]
#
#     cnn.addArticle(cnn_rss['entries'][i]['title'], the_summary, cnn_rss['entries'][i]['link'])
#
# # print(cnn.articles)
#
# # washington post articles
# usa_today_rss = feedparser.parse("http://rssfeeds.usatoday.com/usatoday-NewsTopStories")
#
# for i in range(0, 10):
#     the_title = usa_today_rss['entries'][i]['title'].partition('&')[0]
#
#     usa_today.addArticle(the_title, "n/a", usa_today_rss['entries'][i]['links'][0]['href'])
#
# # print(usa_today.articles)

def readKeywords(filename):
    keyword_file = open(filename, 'r')
    political_keywords = keyword_file.readlines();
    temp = [];

    for line in political_keywords:
        temp.append(line.partition('\n')[0].split(','))
    political_keywords = temp
    keyword_dict = {'Conservative': [], 'Liberal': []}
    for keyword in political_keywords:
        if keyword[1] == 'Liberal':
          keyword_dict['Liberal'].append(keyword[0])
        elif keyword[1] == 'Conservative':
            keyword_dict['Conservative'].append(keyword[0])

    return keyword_dict


political_keyword_dict = readKeywords('politcal_keywords.txt')

print(political_keyword_dict.values())

keyword_list = []

for vals in political_keyword_dict.values():
    for word in vals:
        keyword_list.append(word)

# print(keyword_list)


# copied this from Google Cloud Platform Entity Sentiment Analysis Tutorial and edited it to reflect political keywords and bias
def entity_sentiment_text(text):
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
            print(entity.name, mention.sentiment.magnitude, mention.sentiment.score)
            if entity.name in keyword_list:
                if entity.name in list(political_keyword_dict.values())[1] and mention.sentiment.score > 0:
                    liberal_score += abs(mention.sentiment.score)
                    liberal_words += 1
                elif entity.name in list(political_keyword_dict.values())[0] and mention.sentiment.score < 0:
                    liberal_score += mention.sentiment.score
                    liberal_words += 1
                elif entity.name in list(political_keyword_dict.values())[0] and mention.sentiment.score > 0:
                    conservative_score += mention.sentiment.score
                    conservative_words += 1
                elif entity.name in list(political_keyword_dict.values())[1] and mention.sentiment.score < 0:
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
        return "Moderate"
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
def political_bias(filename):
    file = open(filename, 'r')
    print(entity_sentiment_text(''.join(file.readlines())))


political_bias("test_article")


