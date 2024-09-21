import React from "react";

import {
  InputContainer,
  ItemsContainer,
  NavbarContainer,
  SearchContainer,
  WrapperContainer,
  ItemContainer,
  SearchOutlinedIcon,
  LanguageOutlinedIcon,
  FullscreenExitOutlinedIcon,
  NotificationsNoneOutlinedIcon,
  ChatBubbleOutlineOutlinedIcon,
  ListOutlinedIcon,
  Avatar,
} from "./styles/navbar";

const Navbar = () => {
  return (
    <NavbarContainer>
      <WrapperContainer>
        <SearchContainer>
          <InputContainer type="text" placeholder="Search..." />
          <SearchOutlinedIcon />
        </SearchContainer>
        <ItemsContainer>
          <ItemContainer>
            <LanguageOutlinedIcon className="icon" />
            English
          </ItemContainer>

          <ItemContainer>
            <FullscreenExitOutlinedIcon className="icon" />
          </ItemContainer>
          <ItemContainer>
            <NotificationsNoneOutlinedIcon className="icon" />
            <div className="counter">1</div>
          </ItemContainer>
          <ItemContainer>
            <ChatBubbleOutlineOutlinedIcon className="icon" />
            <div className="counter">2</div>
          </ItemContainer>
          <ItemContainer>
            <ListOutlinedIcon className="icon" />
          </ItemContainer>
          <ItemContainer>
            <Avatar
              src="https://images.pexels.com/photos/941693/pexels-photo-941693.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500"
              alt=""
            />
          </ItemContainer>
        </ItemsContainer>
      </WrapperContainer>
    </NavbarContainer>
  );
};

export default Navbar;
