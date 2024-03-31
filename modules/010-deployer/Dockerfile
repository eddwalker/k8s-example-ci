ARG OS_VERSION

FROM ${OS_VERSION} as base
ENV DEBIAN_FRONTEND noninteractive
ENV PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/snap/bin
COPY modules /modules
COPY libs /libs
COPY self-signed.crt /self-signed.crt
RUN bash -c "./modules/*-deployer/build.sh"

FROM ${OS_VERSION} as modules
COPY --from=base /modules /modules
COPY --from=base /libs /libs
COPY --from=base /usr/local/bin /usr/local/bin
COPY --from=base /self-signed.crt /self-signed.crt

CMD ["/bin/bash"]
