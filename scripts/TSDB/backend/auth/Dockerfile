FROM python:3.11-slim

WORKDIR /app

# system deps for bcrypt and crypto
RUN apt-get update \
    && apt-get install -y --no-install-recommends build-essential libssl-dev libffi-dev sqlite3 \
    && rm -rf /var/lib/apt/lists/*

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY . /app

RUN mkdir -p /app/data && chmod +x /app/entrypoint.sh

ENV PYTHONUNBUFFERED=1
EXPOSE 8001

ENTRYPOINT ["/app/entrypoint.sh"]
