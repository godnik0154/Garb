import React from "react";
import SideBar from "../../components/sidebar";
import Navbar from "../../components/navbar";
import { useState } from "react";
import {
  NewContainer,
  NewContainerContainer, 
  FormContainer,
  FormInputContainer,
  H1Container,
  DriveFolderUploadOutlinedIcon,
  ImageContainer,
  InputContainer,
  LabelContainer,
  LeftContainer,
  RightContainer,
  TopBottomContainer,
  ButtonContainer,
} from "./styles/new";
const New = ({ inputs, title }) => {
  const [file, setFile] = useState("");

  return (
    <NewContainer>
      <SideBar />
      <NewContainerContainer>
        <Navbar />
        <TopBottomContainer>
          <H1Container>Create New Inventory item</H1Container>
        </TopBottomContainer>
        <TopBottomContainer>
          <LeftContainer>
            <ImageContainer
              src={
                file
                  ? URL.createObjectURL(file)
                  : "https://icon-library.com/images/no-image-icon/no-image-icon-0.jpg"
              }
              alt=""
            />
          </LeftContainer>
          <RightContainer>
            <FormContainer>
              <FormInputContainer>
                <LabelContainer htmlFor="file">
                  Image: <DriveFolderUploadOutlinedIcon />
                </LabelContainer>
                <InputContainer
                  type="file"
                  id="file"
                  onChange={(e) => setFile(e.target.files[0])}
                  style={{ display: "none" }}
                />
              </FormInputContainer>

              {inputs.map((input) => (
                <FormInputContainer key={input.id}>
                  <LabelContainer>{input.label}</LabelContainer>
                  <InputContainer
                    type={input.type}
                    placeholder={input.placeholder}
                  />
                </FormInputContainer>
              ))}
              <ButtonContainer>Add</ButtonContainer>
            </FormContainer>
          </RightContainer>
        </TopBottomContainer>
      </NewContainerContainer>
    </NewContainer>
  );
};

export default New;
