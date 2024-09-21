import styled from "styled-components/macro";
import { Link as ReachRouterLink } from "react-router-dom";
import { LocalShipping, InsertChart } from "@mui/icons-material";
import {
  NotificationsNone,
  AccountCircleOutlined,
  SettingsApplications,
  ExitToApp,
  Store,
  CreditCard,
  PersonOutline,
  Dashboard,
} from "@mui/icons-material";

export const SidebarContainer = styled.div`
  flex: 1;
  border-right: 0.5px solid rgb(230, 227, 227);
  min-height: 100vh;
  background-color: #fefbe7;
`;

export const TopContainer = styled.div`
  height: 50px;
  display: flex;
  align-items: center;
  justify-content: center;
`;

export const LinkContainer = styled(ReachRouterLink)`
  text-decoration: none;
`;

export const Logo = styled.span`
  font-size: 20px;
  font-weight: bold;
  color: #6439ff;
`;

export const Divider = styled.hr`
  height: 0;
  border: 0.5px solid rgb(230, 227, 227);
`;

export const CenterContainer = styled.div`
  padding-left: 10px;
  padding-right: 10px;
`;

export const UnorderedList = styled.ul`
  list-style: none;
  margin: 0;
  padding: 0;
`;

export const Title = styled.p`
  font-size: 10px;
  font-weight: bold;
  color: #999;
  margin-top: 15px;
  margin-bottom: 5px;
`;

export const ListElement = styled.li`
  display: flex;
  align-items: center;
  padding: 5px;
  cursor: pointer;

  &:hover {
    background-color: #ece8ff;
  }

  .icon {
    font-size: 18px;
    color: #7451f8;
  }
`;

export const SpanContainer = styled.span`
  font-size: 13px;
  font-weight: 600;
  color: #888;
  margin-left: 10px;
`;

export const LocalShippingIcon = styled(LocalShipping)`
  font-size: 18px;
  color: #7451f8;
`;

export const AccountCircleOutlinedIcon = styled(AccountCircleOutlined)`
  font-size: 18px;
  color: #7451f8;
`;

export const NotificationsNoneIcon = styled(NotificationsNone)`
  font-size: 18px;
  color: #7451f8;
`;

export const SettingsApplicationsIcon = styled(SettingsApplications)`
  font-size: 18px;
  color: #7451f8;
`;

export const InsertChartIcon = styled(InsertChart)`
  font-size: 18px;
  color: #7451f8;
`;

export const ExitToAppIcon = styled(ExitToApp)`
  font-size: 18px;
  color: #7451f8;
`;

export const StoreIcon = styled(Store)`
  font-size: 18px;
  color: #7451f8;
`;

export const CreditCardIcon = styled(CreditCard)`
  font-size: 18px;
  color: #7451f8;
`;

export const PersonOutlineIcon = styled(PersonOutline)`
  font-size: 18px;
  color: #7451f8;
`;

export const DashboardIcon = styled(Dashboard)`
  font-size: 18px;
  color: #7451f8;
`;
