FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

# Install Python and PlatformIO
RUN apt-get update && apt-get install -y python3 python3-pip python3-venv

# Create a virtual environment and add it to path
RUN python3 -m venv /opt/pio-venv
ENV PATH="/opt/pio-venv/bin:$PATH"

# install PlatformIO
RUN pip install platformio

# Pre-download ESP8266 platform (to speed up first build)
RUN pio platform install espressif8266

# Set working directory
WORKDIR /workspace