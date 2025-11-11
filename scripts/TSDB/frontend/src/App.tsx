import { Routes, Route, Navigate } from "react-router-dom";
import Home from "./Pages/Home/Home";
import Auth from "./Pages/Auth/Auth";
import { useAuth } from "./contexts/AuthContext";
import "./App.css";

function App() {
  const { user, isLoading } = useAuth();

  if (isLoading) {
    return <div>Loading...</div>;
  }

  return (
    <div className="app">
      <Routes>
        <Route path="/auth" element={user ? <Navigate to="/home" /> : <Auth />} />
        <Route path="/" element={<Home />} />
        <Route path="/home" element={<Home />} />
      </Routes>
    </div>
  );
}

export default App;
