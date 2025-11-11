#!/bin/bash

# Initialize Let's Encrypt certificates for nginx

if [ -z "$1" ]; then
    echo "Usage: ./init-letsencrypt.sh yourdomain.com [email@example.com]"
    echo ""
    echo "Example: ./init-letsencrypt.sh racecar.macformula.com admin@macformula.com"
    exit 1
fi

DOMAIN=$1
EMAIL=${2:-""}
STAGING=${3:-0} # Set to 1 if you're testing

echo "### Setting up Let's Encrypt for $DOMAIN"

# Create directories
mkdir -p nginx/certbot/conf
mkdir -p nginx/certbot/www

# Download recommended TLS parameters
if [ ! -e "nginx/certbot/conf/options-ssl-nginx.conf" ] || [ ! -e "nginx/certbot/conf/ssl-dhparams.pem" ]; then
    echo "### Downloading recommended TLS parameters ..."
    curl -s https://raw.githubusercontent.com/certbot/certbot/master/certbot-nginx/certbot_nginx/_internal/tls_configs/options-ssl-nginx.conf > "nginx/certbot/conf/options-ssl-nginx.conf"
    curl -s https://raw.githubusercontent.com/certbot/certbot/master/certbot/certbot/ssl-dhparams.pem > "nginx/certbot/conf/ssl-dhparams.pem"
    echo
fi

# Create a temporary nginx config without SSL for initial setup
echo "### Creating temporary nginx config for initial certificate request..."
cat > nginx/nginx.conf.tmp << 'EOF'
events {
    worker_connections 1024;
}

http {
    server {
        listen 80;
        server_name DOMAIN_PLACEHOLDER;

        location /.well-known/acme-challenge/ {
            root /var/www/certbot;
        }

        location / {
            return 200 "Server is being configured...";
            add_header Content-Type text/plain;
        }
    }
}
EOF

sed -i "s/DOMAIN_PLACEHOLDER/$DOMAIN/g" nginx/nginx.conf.tmp

# Backup original config
if [ -f "nginx/nginx.conf" ]; then
    cp nginx/nginx.conf nginx/nginx.conf.bak
fi

# Use temporary config
cp nginx/nginx.conf.tmp nginx/nginx.conf

echo "### Starting nginx with temporary config..."
docker compose -f docker-compose-prod.yml up -d nginx

echo "### Waiting for nginx to start..."
sleep 5

echo "### Requesting Let's Encrypt certificate for $DOMAIN ..."

# Compose the domain arg
DOMAIN_ARGS="-d $DOMAIN -d www.$DOMAIN"

# Select appropriate email arg
EMAIL_ARG=""
if [ -n "$EMAIL" ]; then
    EMAIL_ARG="--email $EMAIL"
else
    EMAIL_ARG="--register-unsafely-without-email"
fi

# Enable staging mode if needed
STAGING_ARG=""
if [ $STAGING != "0" ]; then
    STAGING_ARG="--staging"
fi

docker compose -f docker-compose-prod.yml run --rm certbot certonly --webroot -w /var/www/certbot \
    $STAGING_ARG \
    $EMAIL_ARG \
    $DOMAIN_ARGS \
    --rsa-key-size 4096 \
    --agree-tos \
    --force-renewal

if [ $? -eq 0 ]; then
    echo "### Certificate obtained successfully!"
    
    # Update nginx config with the actual domain
    if [ -f "nginx/nginx.conf.bak" ]; then
        cp nginx/nginx.conf.bak nginx/nginx.conf
    fi
    
    sed -i "s/yourdomain.com/$DOMAIN/g" nginx/nginx.conf
    
    echo "### Reloading nginx with SSL configuration..."
    docker compose -f docker-compose-prod.yml restart nginx
    
    echo ""
    echo "### Setup complete! Your site should now be accessible at:"
    echo "    https://$DOMAIN"
    echo "    https://www.$DOMAIN"
    echo ""
    echo "Grafana: https://$DOMAIN/grafana/"
    echo "Backend API: https://$DOMAIN/api/"
    echo "Auth API: https://$DOMAIN/auth/"
else
    echo "### Certificate request failed!"
    echo "### Restoring original nginx config..."
    if [ -f "nginx/nginx.conf.bak" ]; then
        cp nginx/nginx.conf.bak nginx/nginx.conf
    fi
    exit 1
fi
