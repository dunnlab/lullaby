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
docker run -e PARTICLE_API_KEY=${PARTICLE_API_KEY} -e WEBHOOK_URL=${WEBHOOK_URL} -p 9090:9090 lullaby

```