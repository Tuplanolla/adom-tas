rm -f ~/.adom.data/.HISCORE
rm -f ~/.adom.data/.adom.prc
rm -rf ~/.adom.data/tmpdat/*
cp `dirname $0`/adom.cnt ~/.adom.data/.adom.cnt
cp `dirname $0`/adom.cfg ~/.adom.data/.adom.cfg
cp `dirname $0`/adom.kbd ~/.adom.data/.adom.kbd
`dirname $0`/adom
