import React from "react";
import { SideBar, NavBar, Featured, Charts } from "../../components";
import {
  HomeContainer,
  HomeInnerContainer,
  ChartsContainer,
} from "./styles/home";

const Home = () => {
  return (
    <HomeContainer>
      <SideBar />
      <HomeInnerContainer>
        <NavBar />
        <ChartsContainer>
          <Featured />
          <Charts title="Last 6 Months (Requests)" aspect={2 / 1} />
        </ChartsContainer>
      </HomeInnerContainer>
    </HomeContainer>
  );
};

export default Home;
