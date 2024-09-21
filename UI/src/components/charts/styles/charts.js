import styled from "styled-components/macro";
import { CartesianGrid } from "recharts";

export const ChartContainer = styled.div`
  flex: 4;
  -webkit-box-shadow: 2px 4px 10px 1px rgba(0, 0, 0, 0.47);
  box-shadow: 2px 4px 10px 1px rgba(201, 201, 201, 0.47);
  padding: 10px;
  color: gray;
`;

export const Title = styled.div`
  margin-bottom: 20px;
`;

export const ChartGrid = styled(CartesianGrid)`
  stroke: rgb(228, 225, 225);
`;
