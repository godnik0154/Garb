import React, { useState, useEffect } from "react";
import { useNavigate } from "react-router-dom";
import { Link } from "react-router-dom";
import {
  DataTableContainer,
  DataTableTitle,
  DataTableLink,
  CellAction,
  ViewButton,
  DeleteButton,
  CellWithImage,
  CellImage,
  CellWithStatus,
} from "./styles/requestdatatable";
import { DataGrid } from "@mui/x-data-grid";
import axios from "axios";

const userColumns = [
  { field: "id", headerName: "ID", width: 70 },
  {
    field: "user",
    headerName: "Title",
    width: 230,
    renderCell: (params) => {
      return (
        <CellWithImage>
          <CellImage src={params.row.img} alt="avatar" />
          {params.row.name}
        </CellWithImage>
      );
    },
  },
  {
    field: "project_owner",
    headerName: "Project Owner",
    width: 230,
  },

  {
    field: "quantity",
    headerName: "Quantity",
    width: 100,
  },
  {
    field: "status",
    headerName: "Status",
    width: 160,
    renderCell: (params) => {
      return (
        <CellWithStatus className={`${params.row.status}`}>
          {params.row.status}
        </CellWithStatus>
      );
    },
  },
];

// //temporary data
// const userRows = [
//   {
//     id: 1,
//     name: "Request 1",
//     img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
//     status: "pending",
//     email: "Project Owner 1",
//     quantity: 35,
//   },
//   {
//     id: 2,
//     name: "Request 2",
//     img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
//     email: "Project Owner 2",
//     status: "pending",
//     quantity: 42,
//   },
//   {
//     id: 3,
//     name: "Request 3",
//     img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
//     email: "Project Owner 3",
//     status: "pending",
//     quantity: 45,
//   },
//   {
//     id: 4,
//     name: "Request 4",
//     img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
//     email: "Project Owner 4",
//     status: "pending",
//     quantity: 16,
//   },
//   {
//     id: 5,
//     name: "Request 5",
//     img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
//     email: "Project Owner 5",
//     status: "pending",
//     quantity: 22,
//   },
//   {
//     id: 6,
//     name: "Request 6",
//     img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
//     email: "Project Owner 6",
//     status: "pending",
//     quantity: 15,
//   },
//   {
//     id: 7,
//     name: "Request 7",
//     img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
//     email: "Project Owner 7",
//     status: "pending",
//     quantity: 44,
//   },
//   {
//     id: 8,
//     name: "Request 8",
//     img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
//     email: "Project Owner 8",
//     status: "pending",
//     quantity: 36,
//   },
//   {
//     id: 9,
//     name: "Request 9",
//     img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
//     email: "Project Owner 9",
//     status: "pending",
//     quantity: 65,
//   },
//   {
//     id: 10,
//     name: "Request 10",
//     img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
//     email: "Project Owner 10",
//     status: "pending",
//     quantity: 65,
//   },
// ];

const RequestDataTable = () => {
  const [data, setData] = useState("");
  const navigate = useNavigate();

  const delete_data = async (e) => {
    console.log(e);
    await axios.post("http://172.20.1.78:5000/api/request/delete", {
      id: e.toString(),
    });

    navigate("/requests");
  };
  useEffect(() => {
    let doWork = async () => {
      const result = await axios.get(
        "http://172.20.1.78:5000/api/request/view"
      );
      // console.log(result);
      if (result.status === 200) {
        let new_data = result.data.map((item) => {
          return {
            id: item._id.$oid,
            ...item,
            img: "https://images.pexels.com/photos/1820770/pexels-photo-1820770.jpeg?auto=compress&cs=tinysrgb&dpr=2&w=500",
          };
        });
        console.log(new_data);
        setData(new_data);
      }
    };

    doWork();
  }, []);

  const actionColumn = [
    {
      field: "action",
      headerName: "Action",
      width: 200,
      renderCell: (params) => {
        return (
          <CellAction>
            <Link
              to={"/requests/" + params.row.id}
              style={{ textDecoration: "none" }}
            >
              <ViewButton>View</ViewButton>
            </Link>
            <DeleteButton onClick={() => delete_data(params.row.request_id)}>
              Delete
            </DeleteButton>
          </CellAction>
        );
      },
    },
  ];
  return (
    <DataTableContainer>
      <DataTableTitle>
        Requests
        <DataTableLink to="/requests/new">Add New</DataTableLink>
      </DataTableTitle>
      <DataGrid
        rows={data}
        columns={userColumns.concat(actionColumn)}
        pageSize={9}
        rowsPerPageOptions={[9]}
        checkboxSelection
      />
    </DataTableContainer>
  );
};

export default RequestDataTable;
