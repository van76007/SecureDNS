!/bin/bash

echo "Test started\n"
serverIP="127.0.0.1"

for i in {0..200}
do
dig @$serverIP -p 20053 reciperewards.aavalue.com &
dig @$serverIP -p 20053 reciprocitybird.com &
dig @$serverIP -p 20053 reckless.co.uk &
dig @$serverIP -p 20053 redhotautos.com
dig @$serverIP -p 20053 redhotbux.com &
dig @$serverIP -p 20053 redhotchilli.co.uk
dig @$serverIP -p 20053 redhotwonders.com &
dig @$serverIP -p 20053 redhousebooks.co.uk &
dig @$serverIP -p 20053 redhouseit.co.uk
dig @$serverIP -p 20053 redir-yandex.net &
dig @$serverIP -p 20053 redir.adsonar.com &
dig @$serverIP -p 20053 redir.domaingateway.com
dig @$serverIP -p 20053 redir.ebumna.net &
dig @$serverIP -p 20053 redir.overture.com &
dig @$serverIP -p 20053 redir.sc8.hotbot.com &
dig @$serverIP -p 20053 redir.speedbit.com
dig @$serverIP -p 20053 redir.ws &
dig @$serverIP -p 20053 redir1504.com &
dig @$serverIP -p 20053 redir1805.com &
dig @$serverIP -p 20053 redir2304.com &
dig @$serverIP -p 20053 redir2404.com &
dig @$serverIP -p 20053 redir3105.com &
dig @$serverIP -p 20053 redir94.com &
dig @$serverIP -p 20053 redirad.de &
dig @$serverIP -p 20053 redirect-script.com &
dig @$serverIP -p 20053 redirect.clickshield.net &
dig @$serverIP -p 20053 redirect.fairfax.com.au &
dig @$serverIP -p 20053 redirect.mirarsearch.com &
dig @$serverIP -p 20053 redirect.tracking202.com &
dig @$serverIP -p 20053 redirect.trafficz.com &
dig @$serverIP -p 20053 redirect.virtumundo.com &
dig @$serverIP -p 20053 redirect.webnowa.com &
+dig @$serverIP -p 20053 redirect1.boom.ru &
dig @$serverIP -p 20053 redirect123.awardspace.com &
dig @$serverIP -p 20053 redirectcounter1.com
dig @$serverIP -p 30053 zokuzokusurune.com &
dig @$serverIP -p 30053 torrentz.eu &
dig @$serverIP -p 30053 zsotworkow.pl &
dig @$serverIP -p 30053 zbshkhobufcshi.pillsni.ru &
done

echo "Test finished\n"
