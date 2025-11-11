import React from "react";
import { useAuth } from "../../contexts/AuthContext";
import { useNavigate } from "react-router-dom";
import styles from "./Navbar.module.css";
import macfeLogo from "../../assets/macfe_logo.png";

const Navbar: React.FC = () => {
  const { user, logout } = useAuth();
  const navigate = useNavigate();

  const handleLogout = () => {
    logout();
    navigate('/auth');
  };

  return (
    <nav className={styles.navbar}>
      <div className={styles["navbar-content"]}>
        <div className={styles["navbar-left"]}>
          <img
            src={macfeLogo}
            alt="MACFE Logo"
            className={styles["navbar-logo"]}
          />
          <h1 className={styles["navbar-title"]}>MACFE</h1>
        </div>
        <div className={styles["navbar-right"]}>
          {user ? (
            <>
              <span className={styles["navbar-user"]}>
                {user.username} (Level {user.authorization})
              </span>
              <button onClick={handleLogout} className={styles["logout-button"]}>
                Logout
              </button>
            </>
          ) : (
            <button onClick={() => navigate('/auth')} className={styles["logout-button"]}>
              Login
            </button>
          )}
        </div>
      </div>
    </nav>
  );
};

export default Navbar;
