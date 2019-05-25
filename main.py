import feedparser


class News:
    def __init__(self, name):
        self.name = name
        self.articles = {}

    # parameters are all strings
    def addArticle(self, title, summary, url):
        self.articles[title] = [summary, url]


# news outlets
cnn = News("CNN")

usa_today = News("USA Today")


# cnn articles
cnn_rss = feedparser.parse("http://rss.cnn.com/rss/cnn_topstories.rss")

for i in range(0, 10):
    the_summary = cnn_rss['entries'][i]['summary'].partition('<')[0]

    cnn.addArticle(cnn_rss['entries'][i]['title'], the_summary, cnn_rss['entries'][i]['link'])

print(cnn.articles)

# washington post articles
usa_today_rss = feedparser.parse("http://rssfeeds.usatoday.com/usatoday-NewsTopStories")

for i in range(0, 10):
    the_title = usa_today_rss['entries'][i]['title'].partition('&')[0]

    usa_today.addArticle(the_title, "n/a", usa_today_rss['entries'][i]['links'][0]['href'])

print(usa_today.articles)
