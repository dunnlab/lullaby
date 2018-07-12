FROM ubuntu:bionic
LABEL maintainer="Ben Evans <b.evans@yale.edu"

# to allow specifying versions at buildtime as cli args
ARG prom_version=2.3.1
ARG alertmanager_version=0.15.0
ENV env_prom_version=$prom_version
ENV env_alertmanager_version=$alertmanager_version

RUN apt-get update && apt-get -y install git \
                                         less \
                                         python3 \
    				                             python3-pip \
                                         vim \
    				                             wget

RUN wget https://github.com/prometheus/prometheus/releases/download/v${env_prom_version}/prometheus-${env_prom_version}.linux-amd64.tar.gz && \
  tar xf prometheus-${env_prom_version}.linux-amd64.tar.gz && \
  cd prometheus-${env_prom_version}.linux-amd64 && \
  cp prometheus /bin/prometheus && \
  cp promtool /bin/promtool && \
  cp -r console_libraries /etc/prometheus/ && \
  cp -r consoles /etc/prometheus/ && \
  cd .. && rm -rf prometheus-${env_prom_version}.linux-amd64 prometheus-${env_prom_version}.linux-amd64.tar.gz
  
RUN python3 -m pip install prometheus_client

RUN wget https://github.com/prometheus/alertmanager/releases/download/v${env_alertmanager_version}/alertmanager-${env_alertmanager_version}.linux-amd64.tar.gz && \
  tar xf alertmanager-${env_alertmanager_version}.linux-amd64.tar.gz && \
  cd alertmanager-${env_alertmanager_version}.linux-amd64 && \
  cp alertmanager /bin/alertmanager && \
  cp amtool /bin/amtool && \
  cd .. && rm -rf alertmanager-${env_alertmanager_version}.linux-amd64 alertmanager-${env_alertmanager_version}.linux-amd64.tar.gz

COPY prometheus.yml /etc/prometheus/prometheus.yml
COPY alertmanager.yml /etc/alertmanager/alertmanager.yml
COPY notifications.tmpl /etc/alertmanager/notifications.tmpl
COPY alert_rules.yml /etc/prometheus/alert_rules.yml
COPY prometheus_lullaby_exporter.py /bin/prometheus_lullaby_exporter.py
COPY run_lullaby.sh /bin/run_lullaby.sh
RUN chmod +x /bin/run_lullaby.sh

EXPOSE 9090
EXPOSE 9091
EXPOSE 9093
EXPOSE 9500
CMD ["/bin/run_lullaby.sh"]
