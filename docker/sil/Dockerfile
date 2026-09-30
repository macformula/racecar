FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Minimal toolchain for PlatformIO's "native" platform (used by :sil). No
# ARM toolchain or STM32CubeMX is pre-installed - this image targets :sil,
# not HIL/vehicle. Note this is a convenience, not a hard boundary: since
# PlatformIO fetches platform packages on demand, `pio run -e vehicle` would
# still work inside this container given network access (it would just
# download toolchain-gccarmnoneeabi first). Nothing in this image actively
# blocks that; it's simply not provisioned for it.
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        python3 \
        python3-pip \
        git \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Container runs as root (see below) over a bind-mounted repo. On real Linux
# hosts (e.g. GitHub Actions runners, unlike macOS Docker Desktop's virtiofs
# layer, which reports ownership per-accessor), the mounted files are owned
# by a non-root UID, which trips git's "detected dubious ownership" guard
# (CVE-2022-24765) the moment any git command runs against them - including
# scripts/build/generate_git_hash.py, which :sil's build now invokes. This is
# a disposable, single-purpose container only ever pointed at this repo, so
# trusting any mounted directory is safe.
RUN git config --global --add safe.directory '*'

# Ubuntu 24.04's system pip is PEP 668 "externally managed"; this container
# is single-purpose and disposable, so --break-system-packages is safe here.
#
# protobuf/grpcio-tools are needed because PlatformIO's nanopb build step
# (used by :sil's CAN message codegen) shells out to pip for them at build
# time, which would otherwise hit the same PEP 668 restriction.
#
# cantools/Jinja2/numpy/PyYAML are cangen's (scripts/cangen) runtime deps,
# pinned to match scripts/cangen/pyproject.toml - cangen is consumed as a
# raw PlatformIO lib_dep (not pip-installed), so its dependencies must be
# provided by this environment instead.
RUN pip install --no-cache-dir --break-system-packages \
        platformio \
        protobuf \
        grpcio-tools \
        cantools==39.4.5 \
        Jinja2==3.1.5 \
        numpy==1.26.4 \
        PyYAML==6.0.1

WORKDIR /workspace
CMD ["bash"]
