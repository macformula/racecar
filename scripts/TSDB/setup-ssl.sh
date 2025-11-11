#!/bin/bash

# Simple Let's Encrypt setup for sokosam.com

DOMAIN="${1:-sokosam.com}"
EMAIL="${2:-}"

echo "### Setting up SSL for $DOMAIN"

# Create directories
sudo mkdir -p nginx/certbot/conf
sudo mkdir -p nginx/certbot/www

# Download recommended TLS parameters
echo "### Downloading TLS parameters..."
sudo curl -s https://raw.githubusercontent.com/certbot/certbot/master/certbot-nginx/certbot_nginx/_internal/tls_configs/options-ssl-nginx.conf -o nginx/certbot/conf/options-ssl-nginx.conf
sudo curl -s https://raw.githubusercontent.com/certbot/certbot/master/certbot/certbot/ssl-dhparams.pem -o nginx/certbot/conf/ssl-dhparams.pem

# Set proper permissions
sudo chmod -R 755 nginx/certbot

echo "### Requesting certificate from Let's Encrypt..."

# Build email argument
EMAIL_ARG=""
if [ -n "$EMAIL" ]; then
    EMAIL_ARG="--email $EMAIL"
else
    EMAIL_ARG="--register-unsafely-without-email"
fi

# Stop certbot container if running
docker compose -f docker-compose-prod.yml stop certbot 2>/dev/null

# Request certificate
sudo docker run --rm \
    -v "$(pwd)/nginx/certbot/conf:/etc/letsencrypt" \
    -v "$(pwd)/nginx/certbot/www:/var/www/certbot" \
    certbot/certbot certonly \
    --webroot \
    --webroot-path /var/www/certbot \
    $EMAIL_ARG \
    -d $DOMAIN \
    -d www.$DOMAIN \
    --rsa-key-size 4096 \
    --agree-tos \
    --force-renewal \
    --non-interactive

if [ $? -eq 0 ]; then
    echo ""
    echo "### Success! Certificate obtained."
    echo "### Restarting containers..."
    docker compose -f docker-compose-prod.yml restart certbot
    docker compose -f docker-compose-prod.yml restart nginx
    echo ""
    echo "### Your site should now be accessible at:"
    echo "    https://$DOMAIN"
    echo ""
else
    echo "### Certificate request failed!"
    echo "### Make sure:"
    echo "    1. DNS for $DOMAIN and www.$DOMAIN points to this server"
    echo "    2. Ports 80 and 443 are open"
    echo "    3. No firewall is blocking the connection"
    exit 1
fi
