1. Simple request with body
curl -X POST 192.168.32.8:8080/api/turtle/chopper/v1 --http1.1 \
    -H 'id: 1' \
    -H 'position: 123,456,789' \
    -H 'content-type: application/json' \
    -H 'accept: application/json' \
    --data @misc/turtle_post.json
