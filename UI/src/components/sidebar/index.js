import React from "react";

import {
  SidebarContainer,
  TopContainer,
  LinkContainer,
  Logo,
  Divider,
  CenterContainer,
  UnorderedList,
  Title,
  ListElement,
  SpanContainer,
  LocalShippingIcon,
  SettingsApplicationsIcon,
  InsertChartIcon,
  NotificationsNoneIcon,
  AccountCircleOutlinedIcon,
  ExitToAppIcon,
  StoreIcon,
  CreditCardIcon,
  DashboardIcon,
} from "./styles/sidebar";

const SideBar = () => {
  return (
    <SidebarContainer>
      <TopContainer>
        <LinkContainer to="/">
          <Logo>BAAM's Inventory</Logo>
        </LinkContainer>
      </TopContainer>
      <Divider />
      <CenterContainer>
        <UnorderedList>
          <Title>MAIN</Title>
          <LinkContainer to="/">
            <ListElement>
              <DashboardIcon />
              <SpanContainer>Dashboard</SpanContainer>
            </ListElement>
          </LinkContainer>
          <Title>LISTS</Title>
          <LinkContainer to="/items">
            <ListElement>
              <StoreIcon />
              <SpanContainer>Products</SpanContainer>
            </ListElement>
          </LinkContainer>
          <LinkContainer to = "/requests">
          <ListElement>
            <CreditCardIcon />
            <SpanContainer>Request</SpanContainer>
          </ListElement>
          </LinkContainer>
          <ListElement>
            <LocalShippingIcon />
            <SpanContainer>Delivery</SpanContainer>
          </ListElement>
          <Title>USEFUL</Title>
          <ListElement>
            <InsertChartIcon />
            <SpanContainer>Stats</SpanContainer>
          </ListElement>
          <ListElement>
            <NotificationsNoneIcon />
            <SpanContainer>Notifications</SpanContainer>
          </ListElement>
          <ListElement>
            <SettingsApplicationsIcon />
            <SpanContainer>Settings</SpanContainer>
          </ListElement>
          <Title>USER</Title>
          <ListElement>
            <AccountCircleOutlinedIcon />
            <SpanContainer>Profile</SpanContainer>
          </ListElement>
          <ListElement>
            <ExitToAppIcon />
            <SpanContainer>Logout</SpanContainer>
          </ListElement>
        </UnorderedList>
      </CenterContainer>
    </SidebarContainer>
  );
};

export default SideBar;
