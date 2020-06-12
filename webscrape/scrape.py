#!/bin/python2.7
# Matias Lago, Problem 2, Assignment 2, 2019
# pydoc (cli) was used for python reference, as well as slides provided
from urllib2 import *
from bs4 import BeautifulSoup
import re
import string


words = {}
def evaluate(page):
    file = urlopen("https://en.wikipedia.org/wiki/" + page)
    content_html = file.read()
    obj_html = BeautifulSoup(content_html, "html.parser")

    # Remove script/style elements
    for el in obj_html(["script", "style"]):
        el.extract()

    content_text = obj_html.get_text()
    # Create regexp, search for matches
    regex = "[A-Za-z]+"
    for match in re.finditer(regex, content_text):
        current = match.group()
        if words.has_key(current):
            words[current] += 1
        else:
            words[current] = 1

# Lambda in python: lambda [arguments]: return (pydoc lambda)
# Dictionaries use cmp - a comparison function (like in C++/etc)
# Make a lambda expression to use as cmp for sorting the dictionary of words
# This runs once all evaluations are done
def sort_and_print():
    swords = sorted(words, lambda x,y: cmp(words[y], words[x]))

    for i in range(15):
        print words[swords[i]], unicode(swords[i])

if __name__ == "__main__":
    for first in string.uppercase:
        for second in string.uppercase:
            evaluate(first+second)

    sort_and_print()
