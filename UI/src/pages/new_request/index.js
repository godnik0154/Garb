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
} from "./styles/newrequest";
import axios from "axios";

import { useNavigate } from "react-router-dom";

const NewRequest = () => {
  const [file, setFile] = useState("");
  const [title, setTitle] = useState("");
  const [projectOwner, setProjectOwner] = useState("");
  const [assignedManager, setAssignedManager] = useState("");
  const [status, setStatus] = useState("");
  const [itemId, setItemId] = useState("");
  // const [requestId, setRequestId] = useState("");
  const [quantity, setQuantity] = useState("");
  const [expense, setExpense] = useState("");

  const navigate = useNavigate();

  const send_req = (e) => {
    e.preventDefault();
    axios.post("http://172.20.1.78:5000/api/request/add", {
      name: title,
      project_owner: projectOwner,
      assigned_manager: assignedManager,
      status: status,
      item_id: itemId,
      request_id: "1",
      quantity: quantity,
      expense: expense,
    });

    navigate("/requests");
  };

  return (
    <NewContainer>
      <SideBar />
      <NewContainerContainer>
        <Navbar />
        <TopBottomContainer>
          <H1Container>Create New Request</H1Container>
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

              <FormInputContainer>
                <LabelContainer>{"Title"}</LabelContainer>
                <InputContainer
                  type={"text"}
                  placeholder={"Request Title"}
                  value={title}
                  onChange={(e) => setTitle(e.target.value)}
                />
              </FormInputContainer>
              <FormInputContainer>
                <LabelContainer>{"Project Owner"}</LabelContainer>
                <InputContainer
                  type={"text"}
                  placeholder={"Project Owner"}
                  value={projectOwner}
                  onChange={(e) => setProjectOwner(e.target.value)}
                />
              </FormInputContainer>
              <FormInputContainer>
                <LabelContainer>{"Assigned Manager"}</LabelContainer>
                <InputContainer
                  type={"text"}
                  placeholder={"Assigned Manager"}
                  value={assignedManager}
                  onChange={(e) => setAssignedManager(e.target.value)}
                />
              </FormInputContainer>
              <FormInputContainer>
                <LabelContainer>{"Status"}</LabelContainer>
                <InputContainer
                  type={"text"}
                  placeholder={"Pending"}
                  value={status}
                  onChange={(e) => setStatus(e.target.value)}
                />
              </FormInputContainer>
              <FormInputContainer>
                <LabelContainer>{"Item ID"}</LabelContainer>
                <InputContainer
                  type={"number"}
                  placeholder={"Item ID"}
                  value={itemId}
                  onChange={(e) => setItemId(e.target.value)}
                />
              </FormInputContainer>
              <FormInputContainer>
                <LabelContainer>{"Quantity"}</LabelContainer>
                <InputContainer
                  type={"number"}
                  placeholder={"1"}
                  value={quantity}
                  onChange={(e) => setQuantity(e.target.value)}
                />
              </FormInputContainer>
              <FormInputContainer>
                <LabelContainer>{"Expense"}</LabelContainer>
                <InputContainer
                  type={"number"}
                  placeholder={"1000"}
                  value={expense}
                  onChange={(e) => setExpense(e.target.value)}
                />
              </FormInputContainer>
              <ButtonContainer onClick={send_req}>Add</ButtonContainer>
            </FormContainer>
          </RightContainer>
        </TopBottomContainer>
      </NewContainerContainer>
    </NewContainer>
  );
};

export default NewRequest;
