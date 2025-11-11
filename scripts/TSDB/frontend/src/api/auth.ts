// Auth API utilities
const AUTH_API_URL = '/auth';

export interface AuthResponse {
  access_token: string;
  token_type: string;
}

export interface User {
  id: number;
  username: string;
  authorization: number;
}

export const authApi = {
  async register(username: string, password: string): Promise<AuthResponse> {
    const response = await fetch(`${AUTH_API_URL}/register`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, password }),
    });
    
    if (!response.ok) {
      const error = await response.json();
      throw new Error(error.detail || 'Registration failed');
    }
    
    return response.json();
  },

  async login(username: string, password: string): Promise<AuthResponse> {
    const url = `${AUTH_API_URL}/login`;
    console.log('Login URL:', url);
    console.log('Request body:', { username, password });
    
    const response = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ username, password }),
    });
    
    console.log('Response status:', response.status);
    console.log('Response ok:', response.ok);
    
    if (!response.ok) {
      const error = await response.json();
      console.error('Login error:', error);
      throw new Error(error.detail || 'Login failed');
    }
    
    return response.json();
  },

  async getCurrentUser(token: string): Promise<User> {
    const response = await fetch(`${AUTH_API_URL}/me`, {
      headers: { 'Authorization': `Bearer ${token}` },
    });
    
    if (!response.ok) {
      throw new Error('Failed to get user info');
    }
    
    return response.json();
  },
};

// Token storage utilities
export const tokenStorage = {
  getToken(): string | null {
    return localStorage.getItem('auth_token');
  },
  
  setToken(token: string): void {
    localStorage.setItem('auth_token', token);
  },
  
  removeToken(): void {
    localStorage.removeItem('auth_token');
  },
};
