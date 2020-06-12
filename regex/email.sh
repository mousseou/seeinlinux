#!/bin/bash
# Major program 1
# $1 -> N
# $2 -> Pathname

if ! [ "$#" -eq 2 ]; # Ensure 2 arguments, n and path, are supplied
then
	echo "$0 N pathname"
	exit 1
fi


#set up regex for email verification
LOCAL_REGEX="([A-Za-z0-9])(\.?[A-Za-z0-9])+"
DOMAIN_REGEX="(([A-Za-z0-9])((-?[A-Za-z0-9])?)+\.)+"
TOPDOMAIN_REGEX="([A-Za-z])((-?[A-Za-z])?)+"
EMAIL_REGEX=$LOCAL_REGEX@$DOMAIN_REGEX$TOPDOMAIN_REGEX

# generating the 'topemails.txt' file
grep -EIhor $EMAIL_REGEX $2 | sort | uniq -c | sort -grs | head -n $1 > topemails.txt
# generating the 'emails_top_domains.txt' file
TARGETS=`cat topemails.txt | grep -EIho $DOMAIN_REGEX$TOPDOMAIN_REGEX`
echo -n "" > temp.txt
for d in $TARGETS
do
	grep -EIhor $LOCAL_REGEX@$d $2 >> temp.txt
done
cat temp.txt | sort -u > emails_top_domains.txt
rm temp.txt
