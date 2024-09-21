import React from "react";
import { SideBar, NavBar, DataTable } from "../../components";
import { Home, HomeContainer } from "./styles/items";

const Items = () => {
  return (
    <>
      <Home>
        <SideBar />
        <HomeContainer>
          <NavBar />
          <DataTable />
        </HomeContainer>
      </Home>
    </>
  );
};

export default Items;
