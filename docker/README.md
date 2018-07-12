## Building

To build this container, run:
```bash
docker build . -t lullaby
```

## Running

To run the container, listening on port `9090`:
```bash
export PARTICLE_API_KEY="yourkeyhere"
export WEBHOOK_URL="slackwebhookurl"
docker run -d -e PARTICLE_API_KEY=${PARTICLE_API_KEY} -e WEBHOOK_URL=${WEBHOOK_URL} --network host --name lullaby lullaby

```

To see the logs from the container:
```bash
docker logs lullaby
```

To run an interactive session in the container:
```bash
docker exec -it lullaby /bin/bash
```

