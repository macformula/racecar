"""
Auth utilities for backend server to verify JWT tokens from auth service.
"""
import os
from fastapi import Depends, HTTPException, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
from jose import jwt, JWTError
from pydantic import BaseModel

SECRET_KEY = os.getenv("SECRET_KEY", "test")
ALGORITHM = "HS256"

security = HTTPBearer()


class TokenUser(BaseModel):
    username: str
    authorization: int


def verify_token(credentials: HTTPAuthorizationCredentials = Depends(security)) -> TokenUser:
    """Verify JWT token and return user info with authorization level."""
    token = credentials.credentials
    
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        username = payload.get("sub")
        auth_level = payload.get("auth_level", 0)
        
        if username is None:
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Invalid token",
            )
    except JWTError:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid token",
        )
    
    return TokenUser(username=username, authorization=auth_level)


def require_level_5(user: TokenUser = Depends(verify_token)) -> TokenUser:
    """Dependency that requires authorization level 5."""
    if user.authorization < 5:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail=f"Insufficient permissions. Level 5 required, you have level {user.authorization}.",
        )
    return user
