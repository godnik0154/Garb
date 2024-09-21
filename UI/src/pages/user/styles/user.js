import styled from "styled-components/macro";

export const UserContainer = styled.div`
  display: flex;
  width: 100%;
  background-color: #fefbe7;
`;
export const SingleUserContainer = styled.div`
  flex: 6;
`;
export const TopContainer = styled.div`
  padding: 20px;
  display: flex;
  gap: 20px;
`;
export const LeftContainer = styled.div`
  flex: 1;
  -webkit-box-shadow: 2px 4px 10px 1px rgba(0, 0, 0, 0.47);
  box-shadow: 2px 4px 10px 1px rgba(201, 201, 201, 0.47);
  padding: 20px;
  position: relative;
`;
export const EditButtonContainer = styled.div`
  position: absolute;
  top: 0;
  right: 0;
  padding: 5px;
  font-size: 12px;
  color: #7451f8;
  background-color: #7551f818;
  cursor: pointer;
  border-radius: 0px 0px 0px 5px;
`;
export const ItemContainer = styled.div`
  display: flex;
  gap: 20px;
`;
export const ItemImageContainer = styled.img`
  width: 100px;
  height: 100px;
  border-radius: 50%;
  object-fit: cover;
`;

export const DetailsContainer = styled.div``;

export const ItemTitleContainer = styled.h1`
  margin-bottom: 10px;
  color: #555;
`;

export const DetailItemContainer = styled.div`
  margin-bottom: 10px;
  font-size: 14px;
`;

export const ItemKeyContainer = styled.span`
  font-weight: bold;
  color: gray;
  margin-right: 5px;
`;

export const ItemValueContainer = styled.span`
  font-weight: 300;
`;
