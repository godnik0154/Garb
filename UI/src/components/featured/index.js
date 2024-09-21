import { CircularProgressbar } from "react-circular-progressbar";
import "react-circular-progressbar/dist/styles.css";
import {
  KeyboardArrowDown,
  KeyboardArrowUpOutlined,
  MoreVert,
} from "@mui/icons-material";
import {
  FeaturedContainer,
  Top,
  Title,
  Bottom,
  ParaTitle,
  FeaturedChart,
  Summary,
  Item,
  ItemTitle,
  ItemResult,
  ResultAmount,
} from "./styles/featured";

const Featured = () => {
  return (
    <FeaturedContainer>
      <Top>
        <Title>Total Requests</Title>
        <MoreVert fontSize="small" />
      </Top>
      <Bottom>
        <FeaturedChart>
          <CircularProgressbar value={70} text={"70%"} strokeWidth={5} />
        </FeaturedChart>
        <ParaTitle>Total requests made today</ParaTitle>
        <p
          style={{
            fontSize: "30px",
          }}
        >
          420
        </p>
        <p
          style={{
            fontWeight: 300,
            fontSize: "12px",
            color: "gray",
            textAlign: "center",
          }}
        >
          Previous transactions processing. Last payments may not be included.
        </p>
        <Summary>
          <Item>
            <ItemTitle>Target</ItemTitle>
            <ItemResult style={{ color: "green" }}>
              <KeyboardArrowDown fontSize="small" />
              <ResultAmount>12.4k</ResultAmount>
            </ItemResult>
          </Item>
          <Item>
            <ItemTitle>Last Week</ItemTitle>
            <ItemResult style={{ color: "red" }}>
              <KeyboardArrowUpOutlined fontSize="small" />
              <ResultAmount>12.4k</ResultAmount>
            </ItemResult>
          </Item>
          <Item>
            <ItemTitle>Last Month</ItemTitle>
            <ItemResult style={{ color: "green" }}>
              <KeyboardArrowUpOutlined fontSize="small" />
              <ResultAmount>12.4k</ResultAmount>
            </ItemResult>
          </Item>
        </Summary>
      </Bottom>
    </FeaturedContainer>
  );
};

export default Featured;
