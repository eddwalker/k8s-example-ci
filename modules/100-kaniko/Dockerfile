FROM gcr.io/go-containerregistry/crane AS crane

FROM gcr.io/kaniko-project/executor:v1.21.1-debug

COPY --from=crane /ko-app/crane /kaniko/
