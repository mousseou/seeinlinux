#!/bin/bash
# Problem 1, Matias Lago, 2019

echo '' > wordlist.txt # ensure word list file is empty before insertions

for first in {A..Z} 
do
	for second in {A..Z}
	do
		page=$first$second
		wget https://en.wikipedia.org/wiki/$page -O $page.html -q
		lynx -dump -nolist $page.html > $page.txt
		grep -Eow "[[:alpha:]]+" $page.txt >> wordlist.txt
		# remove "trash" files
		rm $page.*
	done
done

# once words are sorted in words.txt or so just run
# sort words | uniq -c | sort -r
# to sort word count in decreasing order

sort wordlist.txt | uniq -c | sort -rn | head -n 15
