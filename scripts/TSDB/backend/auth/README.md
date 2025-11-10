# Auth Service

Small FastAPI authentication service using SQLite and JWT tokens.

How to build and run (from `scripts/TSDB/backend`):

```bash
# build and run via docker-compose
docker compose up --build
```

By default the service listens on port 8001. Endpoints:

- POST /register {"username":"u","password":"p"} -> returns {"access_token":...}
- POST /login {"username":"u","password":"p"} -> returns {"access_token":...}
- GET /me (requires Authorization: Bearer <token>) -> returns user info

The SQLite database file is stored in `auth/data/auth.db` (mounted into the container).
