FROM ubuntu:bionic
LABEL maintainer="Ben Evans <b.evans@yale.edu"

ARG prom_version=2.3.1.linux-amd64
ENV env_prom_version=$prom_version

RUN apt-get update && ap-get install git \
                                     less \
                                     python3 \
                                     vim

RUN wget https://github.com/prometheus/prometheus/releases/download/v2.3.1/prometheus-${env_prom_version}.tar.gz && \
  tar xf prometheus-${env_prom_version}.tar.gz && \
  cd prometheus-${env_prom_version} &&
  cp prometheus /bin/prometheus && \
  cp promtool /bin/promtool && \
  cp -r console_libraries /etc/prometheus/ && \
  cp -r consoles /etc/prometheus/ && \
  cd .. && rm -rf prometheus-${env_prom_version} prometheus-${env_prom_version}.tar.gz
RUN pip3 install prometheus_client


COPY prometheus.yml /etc/prometheus/prometheus.yml
COPY prometheus_lullaby_client.py /bin/prometheus_lullaby_client.py
COPY run_lullaby.sh /bin/run_lullaby.sh

EXPOSE 9090
ENTRYPOINT ["/bin/sh -c"]
CMD ["/bin/run_lullaby.sh"]