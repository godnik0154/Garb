import styled from "styled-components/macro";
import { DriveFolderUploadOutlined } from "@mui/icons-material";

export const NewContainer = styled.div`
  width: 100%;
  display: flex;
  background-color: #fefbe7;
`;

export const NewContainerContainer = styled.div`
  flex: 6;
`;

export const TopBottomContainer = styled.div`
  -webkit-box-shadow: 2px 4px 10px 1px rgba(0, 0, 0, 0.47);
  box-shadow: 2px 4px 10px 1px rgba(201, 201, 201, 0.47);
  padding: 10px;
  margin: 20px;
  display: flex;
`;

export const LeftContainer = styled.div`
  flex: 1;
  text-align: center;
  display: flex;
  align-items: center;
  justify-content: center;
`;

export const H1Container = styled.h1`
  color: #7451f8;
  font-size: 20px;
`;

export const ImageContainer = styled.img`
  width: 100px;
  height: 100px;
  border-radius: 50%;
  object-fit: cover;
`;

export const RightContainer = styled.div`
  flex: 2;
`;

export const FormContainer = styled.form`
  display: flex;
  flex-wrap: wrap;
  gap: 30px;
  justify-content: space-around;
`;

export const FormInputContainer = styled.div`
  width: 40%;
`;

export const LabelContainer = styled.label`
  display: flex;
  align-items: center;
  gap: 10px;
`;

export const DriveFolderUploadOutlinedIcon = styled(DriveFolderUploadOutlined)`
  cursor: pointer;
`;

export const InputContainer = styled.input`
  width: 100%;
  padding: 5px;
  border: none;
  border-bottom: 1px solid gray;
  background-color: #fefbe7;
`;

export const ButtonContainer = styled.button`
  width: 150px;
  padding: 10px;
  border: none;
  background-color: #7451f8;
  color: white;
  font-weight: bold;
  cursor: pointer;
  margin-top: 10px;
`;
