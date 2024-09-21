import styled from "styled-components/macro";
import {
  NotificationsNone,
  AccountCircleOutlined,
  SettingsApplications,
  ExitToApp,
  Store,
  CreditCard,
  PersonOutline,
  Dashboard,
  SearchOutlined,
  LanguageOutlined,
  FullscreenExitOutlined,
  NotificationsNoneOutlined,
  ChatBubbleOutlineOutlined,
  ListOutlined,
} from "@mui/icons-material";

export const NavbarContainer = styled.div`
  height: 50px;
  border-bottom: 0.5px solid rgb(231, 228, 228);
  display: flex;
  align-items: center;
  font-size: 14px;
  color: #555;
  background-color: #fefbe7;
`;

export const WrapperContainer = styled.div`
  width: 100%;
  padding: 20px;
  display: flex;
  align-items: center;
  justify-content: space-between;
`;

export const SearchContainer = styled.div`
  display: flex;
  align-items: center;
  border: 0.5px solid lightgray;
  padding: 3px;
`;

export const InputContainer = styled.input`
  border: none;
  outline: none;
  background: transparent;
  background-color: #fefbe7;

  &::placeholder {
    font-size: 12px;
  }
`;

export const ItemsContainer = styled.div`
  display: flex;
  align-items: center; ;
`;

export const ItemContainer = styled.div`
  display: flex;
  align-items: center;
  margin-right: 20px;
  position: relative;
`;

export const Avatar = styled.img`
  width: 30px;
  height: 30px;
  border-radius: 50%;
`;

export const NotificationsNoneIcon = styled(NotificationsNone)`
  font-size: 20px;
`;

export const SettingsApplicationsIcon = styled(SettingsApplications)`
  font-size: 20px;
`;

export const ExitToAppIcon = styled(ExitToApp)`
  font-size: 20px;
`;

export const StoreIcon = styled(Store)`
  font-size: 20px;
`;

export const CreditCardIcon = styled(CreditCard)`
  font-size: 20px;
`;

export const PersonOutlineIcon = styled(PersonOutline)`
  font-size: 20px;
`;

export const SearchOutlinedIcon = styled(SearchOutlined)`
  font-size: 20px;
`;

export const DashboardIcon = styled(Dashboard)`
  font-size: 20px;
`;

export const AccountCircleOutlinedIcon = styled(AccountCircleOutlined)`
  font-size: 20px;
`;

export const LanguageOutlinedIcon = styled(LanguageOutlined)`
  font-size: 20px;
`;

export const FullscreenExitOutlinedIcon = styled(FullscreenExitOutlined)`
  font-size: 20px;
`;

export const ChatBubbleOutlineOutlinedIcon = styled(ChatBubbleOutlineOutlined)`
  font-size: 20px;
`;

export const ListOutlinedIcon = styled(ListOutlined)`
  font-size: 20px;
`;

export const NotificationsNoneOutlinedIcon = styled(NotificationsNoneOutlined)`
  font-size: 20px;
`;
