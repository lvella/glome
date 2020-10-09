FROM ubuntu:20.04 AS fikalab_ci

ARG DEBIAN_FRONTEND=noninteractive

# install dependencies for glome
RUN apt-get update && \
    apt-get -y install build-essential g++ clang cmake ninja-build \
    libgl1-mesa-dev libglew1.5-dev libsdl2-dev libboost-thread-dev \
    libboost-system-dev libopusfile-dev libopenal-dev && \
    apt-get -y clean

FROM fikalab_ci AS fikalab_developer

# install utilities
RUN apt-get -y install vim sudo git htop

# make bash the default shell
RUN rm /bin/sh && ln -s bash /bin/sh

ENV USER_NAME developer
ENV WORKDIR /home/$USER_NAME
ENV PROJECT glome

ARG host_uid=1001
ARG host_gid=1001
RUN groupadd -g $host_gid $USER_NAME && \
    useradd -g $host_gid -m -s /bin/bash -u $host_uid $USER_NAME

# add user `developer` to sudoers
RUN usermod -aG sudo $USER_NAME

# make all accounts passwordless
RUN echo "ALL   ALL = (ALL) NOPASSWD: ALL" >> /etc/sudoers

USER $USER_NAME
WORKDIR $WORKDIR

# run this command so that docker does not exit immediatly
CMD sleep 1
