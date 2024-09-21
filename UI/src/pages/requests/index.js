import React from "react";
import { SideBar, NavBar, RequestDataTable } from "../../components";
import { Home, HomeContainer } from "./styles/requests"

const Requests = () => {
  return (
    <>
      <Home>
        <SideBar />
        <HomeContainer>
          <NavBar />
          <RequestDataTable />
        </HomeContainer>
      </Home>
    </>
  );
};

export default Requests;
