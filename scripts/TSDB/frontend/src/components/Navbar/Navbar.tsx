import React from "react";
import styles from "./Navbar.module.css";
import macfeLogo from "../../assets/macfe_logo.png";

const Navbar: React.FC = () => {
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
      </div>
    </nav>
  );
};

export default Navbar;
