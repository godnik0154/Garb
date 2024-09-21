import styled from "styled-components/macro";

export const StyledContainer = styled.div`
  display: flex;
  height: 100vh;
  background-color: #7451f8;
  color: beige;
`;

export const CenteredContainer = styled.div`
  margin: 80px 100px 80px 100px;
  width: 100%;
  border: 1px solid beige;
  display: flex;
  flex-direction: row;
  align-items: center;
`;

export const LeftContainer = styled.div`
  display: flex;
  justify-content: center;
  align-items: center;
  border-right: 1px solid beige;
  height: 100%;
  width: 50%;
  background-color: #fefbe7;
`;

export const LoginImage = styled.img`
  height: 400px;
  width: 400px;
`;

export const RightContainer = styled.div`
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  height: 100%;
  width: 50%;
`;

export const RightTitle = styled.h1`
  font-size: 24px;
  padding-top: 10px;
`;

export const FormContainer = styled.div``;

export const InputTitle = styled.h4`
  color: beige;
  font-size: 18px;
  padding-top: 10px;
  padding-left: 210px;
`;

export const Input = styled.input`
  background-color: #7451f8;
  border-bottom: 1px solid beige;
  border-top: none;
  border-right: none;
  border-left: none;
  width: 250px;
  margin-right: 100px;
  margin-left: 210px;
  margin-bottom: 10px;
`;

export const SubmitButton = styled.button`
  background-color: #fff;
  width: 250px;
  height: 50px;
  border: none;
  margin-left: 210px;
  border-radius: 4px;
  color: black;
  font-size: 18px;
  font-weight: 500;
  cursor: pointer;
  
`;

export const LogoImage = styled.img`
  height: 100px;
  width: 100px;
`;

export const Divider = styled.div`
  display: flex;
  flex-direction: row;
  justify-content: center;
  align-items: center;
`;

export const DividerOr = styled.p`
  font-size: 12px;
  padding: 10px;
  &:before,
  &:after {
    height: 1px;
    content: "---------------------------";
    right: 100%;
    left: 100%;
  }
  &:before {
    text-align: right;
  }
`;
