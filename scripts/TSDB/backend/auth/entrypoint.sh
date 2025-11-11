#!/bin/sh
# Initialize admin user directly with SQLite

if [ -z "$ADMIN_PASSWORD" ]; then
    echo "ERROR: ADMIN_PASSWORD environment variable is not set!"
    exit 1
fi

# Generate a bcrypt hash in Python (we need this for password hashing)
HASHED_PASSWORD=$(python3 -c "
from passlib.context import CryptContext
pwd_context = CryptContext(schemes=['bcrypt'], deprecated='auto')
print(pwd_context.hash('$ADMIN_PASSWORD'))
")

# Create database and insert admin user
sqlite3 /app/data/auth.db <<EOF
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username VARCHAR NOT NULL UNIQUE,
    hashed_password VARCHAR NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    authorization INTEGER
);

INSERT OR REPLACE INTO users (id, username, hashed_password, authorization)
VALUES (
    (SELECT id FROM users WHERE username = 'admin'),
    'admin',
    '$HASHED_PASSWORD',
    5
);
EOF

echo "Admin user initialized with authorization level 5"

# Start the server
exec uvicorn app:app --host 0.0.0.0 --port 8001
