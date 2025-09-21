FROM ghcr.io/therainbowphoenix/cp-codespace:beta-02

USER root

RUN echo "" > /etc/apt/sources.list.d/debian.sources
RUN echo "deb http://deb.debian.org/debian bookworm main contrib non-free" > /etc/apt/sources.list && \
    echo "deb http://deb.debian.org/debian-security bookworm-security main contrib non-free" >> /etc/apt/sources.list && \
    echo "deb http://deb.debian.org/debian bookworm-updates main contrib non-free" >> /etc/apt/sources.list

RUN apt-get update && apt-get install -y nano vim clangd-13 clang-format-13 wget

USER dev

RUN mkdir ~/tmp
RUN wget https://raw.githubusercontent.com/ClasspadDev/faq/main/faq.md -O faq

RUN echo -e "echo \"$(tput setaf 2)\n\n [  > v<]~      Welcome to the ClassPad SDK ! \n                You can build using \"make bin\"\n                Check the faq \"cat ~/faq\"\n\n$(tput sgr0)\"" >> ~/.bashrc
