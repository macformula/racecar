from fastapi import FastAPI, Depends, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from sqlalchemy.orm import Session

from database import SessionLocal, init_db
import models
from utils import get_password_hash, verify_password, create_access_token, get_current_user

init_db()

app = FastAPI(title="Auth Service")

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Allows all origins
    allow_credentials=True,
    allow_methods=["*"],  # Allows all methods
    allow_headers=["*"],  # Allows all headers
)


class RegisterIn(BaseModel):
    username: str
    password: str


class TokenOut(BaseModel):
    access_token: str
    token_type: str = "bearer"


def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()


@app.post("/register", response_model=TokenOut)
def register(payload: RegisterIn, db: Session = Depends(get_db)):
    existing = db.query(models.User).filter(models.User.username == payload.username).first()
    if existing:
        raise HTTPException(status_code=400, detail="Username already registered")
    user = models.User(username=payload.username, hashed_password=get_password_hash(payload.password), authorization=1)
    db.add(user)
    db.commit()
    db.refresh(user)
    token = create_access_token({"sub": user.username, "auth_level": user.authorization})
    return {"access_token": token}


@app.post("/login", response_model=TokenOut)
def login(payload: RegisterIn, db: Session = Depends(get_db)):
    user = db.query(models.User).filter(models.User.username == payload.username).first()
    if not user or not verify_password(payload.password, user.hashed_password):
        raise HTTPException(status_code=401, detail="Invalid credentials")
    token = create_access_token({"sub": user.username, "auth_level": user.authorization})
    return {"access_token": token}


@app.get("/me")
def me(current_user=Depends(get_current_user)):
    return {"username": current_user.username, "id": current_user.id, "authorization": current_user.authorization}


if __name__ == "__main__":
    import uvicorn

    uvicorn.run("app:app", host="0.0.0.0", port=8001, reload=True)
