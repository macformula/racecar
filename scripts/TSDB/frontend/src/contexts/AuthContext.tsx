import { createContext, useContext, useState, useEffect } from 'react';
import type { ReactNode } from 'react';
import { authApi, tokenStorage } from '../api/auth';
import type { User } from '../api/auth';

interface AuthContextType {
  user: User | null;
  token: string | null;
  login: (username: string, password: string) => Promise<void>;
  register: (username: string, password: string) => Promise<void>;
  logout: () => void;
  isLoading: boolean;
}

const AuthContext = createContext<AuthContextType | undefined>(undefined);

export function AuthProvider({ children }: { children: ReactNode }) {
  const [user, setUser] = useState<User | null>(null);
  const [token, setToken] = useState<string | null>(null);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    // Check for existing token on mount
    const savedToken = tokenStorage.getToken();
    if (savedToken) {
      authApi.getCurrentUser(savedToken)
        .then(userData => {
          setUser(userData);
          setToken(savedToken);
        })
        .catch(() => {
          tokenStorage.removeToken();
        })
        .finally(() => setIsLoading(false));
    } else {
      setIsLoading(false);
    }
  }, []);

  const login = async (username: string, password: string) => {
    const response = await authApi.login(username, password);
    tokenStorage.setToken(response.access_token);
    const userData = await authApi.getCurrentUser(response.access_token);
    setToken(response.access_token);
    setUser(userData);
  };

  const register = async (username: string, password: string) => {
    const response = await authApi.register(username, password);
    tokenStorage.setToken(response.access_token);
    const userData = await authApi.getCurrentUser(response.access_token);
    setToken(response.access_token);
    setUser(userData);
  };

  const logout = () => {
    tokenStorage.removeToken();
    setToken(null);
    setUser(null);
  };

  return (
    <AuthContext.Provider value={{ user, token, login, register, logout, isLoading }}>
      {children}
    </AuthContext.Provider>
  );
}

export function useAuth() {
  const context = useContext(AuthContext);
  if (context === undefined) {
    throw new Error('useAuth must be used within an AuthProvider');
  }
  return context;
}
