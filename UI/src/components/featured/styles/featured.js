import styled from "styled-components/macro";

export const FeaturedContainer = styled.div`
  flex: 2;
  -webkit-box-shadow: 2px 4px 10px 1px rgba(0, 0, 0, 0.47);
  box-shadow: 2px 4px 10px 1px rgba(201, 201, 201, 0.47);
  padding: 10px;
`;

export const Top = styled.div`
  display: flex;
  align-items: center;
  justify-content: space-between;
  color: gray;
`;

export const Title = styled.h1`
  font-size: 16px;
  font-weight: 500;
`;

export const Bottom = styled.div`
  padding: 20px;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 15px;
`;

export const ParaTitle = styled.p`
  font-size: 16px;
  font-weight: 500;
`;

export const Summary = styled.div`
  width: 100%;
  display: flex;
  align-items: center;
  justify-content: space-between;
`;

export const Item = styled.div`
  text-align: center;
`;

export const FeaturedChart = styled.div`
  width: 100px;
  height: 100px;
`;

export const ItemTitle = styled.div`
  font-size: 14px;
  color: gray;
`;

export const ItemResult = styled.div`
  display: flex;
  align-items: center;
  margin-top: 10px;
  font-size: 14px;
  &.positive {
    color: green;
  }
  &.negative {
    color: red;
  }
`;

export const ResultAmount = styled.div``;
