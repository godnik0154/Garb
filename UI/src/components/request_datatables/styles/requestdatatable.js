import styled from "styled-components/macro";
import { Link } from "react-router-dom";

export const DataTableContainer = styled.div`
  height: 600px;
  padding: 20px;
`;

export const DataTableTitle = styled.div`
  width: 100%;
  font-size: 24px;
  color: gray;
  margin-bottom: 10px;
  display: flex;
  align-items: center;
  justify-content: space-between;
`;

export const DataTableLink = styled(Link)`
  text-decoration: none;
  color: green;
  font-size: 16px;
  font-weight: 400;
  border: 1px solid green;
  padding: 5px;
  border-radius: 5px;
  cursor: pointer;
`;

export const CellAction = styled.div`
  display: flex;
  align-items: center;
  gap: 15px;
`;

export const ViewButton = styled.div`
  padding: 2px 5px;
  border-radius: 5px;
  color: darkblue;
  border: 1px dotted rgba(0, 0, 139, 0.596);
  cursor: pointer;
`;

export const DeleteButton = styled.div`
  padding: 2px 5px;
  border-radius: 5px;
  color: crimson;
  border: 1px dotted rgba(220, 20, 60, 0.6);
  cursor: pointer;
`;

export const CellWithImage = styled.div`
  display: flex;
  align-items: center;
`;

export const CellImage = styled.img`
  width: 32px;
  height: 32px;
  border-radius: 50%;
  object-fit: cover;
  margin-right: 20px;
`;

export const CellWithStatus = styled.div`
  width: 50px;
  height: 20px;
  padding: 5px;
  border-radius: 5px;
  display: flex;
  justify-content: center;
  align-items: center;

  &.active {
    background-color: rgba(0, 128, 0, 0.05);
    color: green;
  }
  &.pending {
    background-color: rgba(255, 217, 0, 0.05);
    color: goldenrod;
  }
  &.passive {
    background-color: rgba(255, 0, 0, 0.05);
    color: crimson;
  }
`;
